# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


import struct
import gdb

# Use the base class if available.
if hasattr(gdb, 'ValuePrinter'):
    printer_base = gdb.ValuePrinter
else:
    printer_base = object

# Use the new style pretty printing.
import gdb.printing


# For each pretty printer, store
# - name       unique identifier of the pretty printer
# - regex      the regular expression used to select the pretty printer
# - printer    the class with the printer definition
class PrinterSetup:
    def __init__(self, name, regex, printer):
        self.name = name
        self.regex = regex
        self.printer = printer


_ocudu_printers = []


############################################
#            Pretty-Printers
############################################

# For each printer, also provide a lookup function and append the set-up
# description to the _ocudu_printers list.

# If you add a pretty printer, please create the corresponding test in
# tests/utils/gdb/pretty_printers

###### static_vector<T, N> ########

class StaticVectorPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_static_vector_test.cpp

    def __init__(self, val):
        self.val = val
        self.value_type = self.val.type.template_argument(0)

    def children(self):
        start = self.val['array']['_M_elems'].cast(self.value_type.pointer())
        length = int(self.val['sz'])
        for idx in range(length):
            yield f'[{idx}]', start[idx]

    def to_string(self):
        length = int(self.val['sz'])
        capacity = int(self.val.type.template_argument(1))
        return f'static_vector of length {length}, capacity {capacity}'

    def display_hint(self):
        return 'array'


_ocudu_printers.append(PrinterSetup('static vector', '^ocudu::static_vector<.*>$', StaticVectorPrinter))


###### bounded_bitset<N, reversed> ########

class BoundedBitsetPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_bounded_bitset_test.cpp

    def __init__(self, val):
        self.val = val

    def to_string(self):
        length = int(self.val['cur_size'])
        capacity = int(self.val.type.template_argument(0))
        buffer = self.val['buffer']['_M_elems']
        bitstring = ''
        nof_words = (length + 63) // 64
        nof_bits_in_word = 64
        for idx in reversed(range(nof_words)):
            bitstring += '{:064b}'.format(int(buffer[idx]))
        # last word might have a lower number of bits
        last_word_nof_bits = length % 64
        bitstring = bitstring[64 - last_word_nof_bits::]
        return f'bounded_bitset of length {length}, capacity {capacity} = {bitstring}'

    def display_hint(self):
        return 'array'


_ocudu_printers.append(PrinterSetup('bounded bitset', '^ocudu::bounded_bitset<.*>$', BoundedBitsetPrinter))


###### tiny_optional<T> #######

class TinyOptionalPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_tiny_optional_test.cpp

    def __init__(self, val):
        self.val = val
        self.has_val = TinyOptionalPrinter.get_has_value(self.val)

    def children(self):
        if self.has_val:
            yield '[0]', TinyOptionalPrinter.get_value(self.val)

    def to_string(self):
        if self.has_val:
            return 'tiny_optional (present)'
        return 'tiny_optional (empty)'

    def display_hint(self):
        return 'array'

    @staticmethod
    def get_has_value(gdb_val):
        fields = gdb_val.type.strip_typedefs().fields()
        assert len(fields) > 0
        f_type_str = str(fields[0].type.strip_typedefs())
        if f_type_str.startswith('std::optional<'):
            return gdb_val['_M_payload']['_M_engaged']
        if 'std::unique_ptr<' in str(gdb_val['val'].type):
            val_str = str(gdb_val['val'])
            val_str = val_str[val_str.find('get() = ') + len('get() = ')::]
            val_str = val_str[0:val_str.find('}')]
            val_int = int(val_str, 16)
            return val_int != 0
        return True  # TODO: tiny_optional with flag

    @staticmethod
    def get_value(gdb_val):
        fields = gdb_val.type.strip_typedefs().fields()
        f_type_str = str(fields[0].type.strip_typedefs())
        if f_type_str.startswith('std::optional<'):
            return gdb_val['_M_payload']['_M_payload']['_M_value']
        return gdb_val['val']


_ocudu_printers.append(PrinterSetup('tiny optional', '^ocudu::tiny_optional<.*>$', TinyOptionalPrinter))


###### slotted_array<T, N> #######

class SlotArrayPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_slotted_array_test.cpp

    def __init__(self, val):
        self.val = val
        self.value_type = self.val.type.strip_typedefs().template_argument(0)
        self.capacity = int(self.val.type.strip_typedefs().template_argument(1))
        self.nof_elems = int(self.val['nof_elems'])

    def children(self):
        opt_value = gdb.lookup_type(f'ocudu::tiny_optional<{self.value_type}>')
        vec_size = self.val['vec']['sz']
        vec = self.val['vec']['array']['_M_elems'].cast(opt_value.pointer())
        count = 0
        for idx in range(vec_size):
            if TinyOptionalPrinter.get_has_value(vec[idx]):
                yield f'{count}', f'{idx}'
                count += 1
                yield f'{count}', TinyOptionalPrinter.get_value(vec[idx])
                count += 1

    def to_string(self):
        return f'slotted_array of {self.nof_elems} elements, capacity {self.capacity}'

    def display_hint(self):
        return 'map'


_ocudu_printers.append(PrinterSetup('slotted array', '^ocudu::slotted_array<.*>$', SlotArrayPrinter))


###### slotted_vector<T> #######

class SlotVectorPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_slotted_vector_test.cpp

    def __init__(self, val):
        self.val = val
        self.value_type = self.val.type.strip_typedefs().template_argument(0)
        self.objects = self.val['objects']
        self.nof_elems = int(self.objects['_M_impl']['_M_finish'] - self.objects['_M_impl']['_M_start'])

    def children(self):
        indexmapper = self.val['index_mapper']
        nof_idxs = int(indexmapper['_M_impl']['_M_finish'] - indexmapper['_M_impl']['_M_start'])
        max_int = 2 ** 64 - 1
        indexmapper_ptr = indexmapper['_M_impl']['_M_start']
        object_ptr = self.objects['_M_impl']['_M_start']
        count = 0
        for idx in range(nof_idxs):
            if int(indexmapper_ptr[idx]) != max_int:
                yield f'{count}', f'{idx}'
                count += 1
                yield f'{count}', object_ptr[indexmapper_ptr[idx]]
                count += 1

    def to_string(self):
        return f'slotted_vector of {self.nof_elems} elements'

    def display_hint(self):
        return 'map'


_ocudu_printers.append(PrinterSetup('slotted vector', '^ocudu::slotted_vector<.*>$', SlotVectorPrinter))


###### Brain Floating Point 16 (bf16_t) ######

class BFloat16Printer(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_bf16_test.cpp

    def __init__(self, val):
        self.__val = val

    def to_string(self):
        value_uint16 = self.__val['val']
        value_uint32 = value_uint16.cast(gdb.lookup_type('uint32_t')) << 16
        value_float = struct.unpack('!f', struct.pack('!I', value_uint32))[0]
        return value_float

    def display_hint(self):
        return None


_ocudu_printers.append(PrinterSetup('bf16', 'ocudu::strong_bf16_tag', BFloat16Printer))


class BFloat16ComplexPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_cbf16_test.cpp

    def __init__(self, val):
        self.__val = val

    def to_string(self):
        return f'{self.__val["real"]} + {self.__val["imag"]}i'

    def display_hint(self):
        return None


_ocudu_printers.append(PrinterSetup('complex bf16', '^ocudu::cbf16_t$', BFloat16ComplexPrinter))


######  tl::expected<T, E> ######

class TlExpectedPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_tl_expected_test.cpp

    def __init__(self, val):
        self.__val = val

    def children(self):
        if self.__val['m_has_val']:
            yield '[0]', self.__val['m_val']
        else:
            yield '[0]', self.__val['m_unexpect']['m_val']

    def to_string(self):
        if self.__val['m_has_val']:
            return 'expected (has value)'
        return 'expected (has error)'

    def display_hint(self):
        return 'array'


_ocudu_printers.append(PrinterSetup('tl expected', '^tl::expected<.*>$', TlExpectedPrinter))


######  byte_buffer / byte_buffer_view ######

# Helper to format byte_buffer, byte_buffer_slice and byte_buffer_view.
def _format_byte_buffer_segment_list(type_name, node, start_offset, total_len):
    if total_len == 0:
        return f'{type_name} of length 0'
    hex_bytes = []
    remaining = total_len
    first = True
    while int(node) != 0 and remaining > 0:
        seg = node.dereference()
        payload_ptr = seg['payload']['ptr']
        payload_len = int(seg['payload']['len'])
        seg_offset = start_offset if first else 0
        first = False
        count = min(payload_len - seg_offset, remaining)
        for i in range(count):
            hex_bytes.append(f'{int(payload_ptr[seg_offset + i]):02x}')
        remaining -= count
        node = seg['next']
    return f'{type_name} of length {total_len} = [{" ".join(hex_bytes)}]'


class ByteBufferPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_byte_buffer_test.cpp

    def __init__(self, val):
        self.__val = val

    def to_string(self):
        type_name = 'byte_buffer'
        ctrl_blk = self.__val['ctrl_blk_ptr']['ptr']
        if int(ctrl_blk) == 0:
            return f'{type_name} of length 0'
        pkt_len = int(ctrl_blk.dereference()['pkt_len'])
        node = ctrl_blk.dereference()['segments']['head']
        return _format_byte_buffer_segment_list(type_name, node, 0, pkt_len)

    def display_hint(self):
        return None


_ocudu_printers.append(PrinterSetup('byte buffer', '^ocudu::byte_buffer$', ByteBufferPrinter))


# Format a byte_buffer_view/byte_buffer_slice (with 'it' and 'it_end' fields).
def _format_byte_buffer_view(type_name, view_val):
    it = view_val['it']
    it_end = view_val['it_end']
    node = it['current_segment']
    if int(node) == 0:
        return f'{type_name} of length 0'
    # Compute total length by walking segments from it to it_end.
    start_offset = int(it['offset'])
    node_end = it_end['current_segment']
    end_offset = int(it_end['offset'])
    total_len = 0
    cur = node
    first = True
    while int(cur) != 0:
        seg = cur.dereference()
        seg_offset = start_offset if first else 0
        first = False
        if int(cur) == int(node_end):
            total_len += end_offset - seg_offset
            break
        total_len += int(seg['payload']['len']) - seg_offset
        cur = seg['next']
    # Format buffer.
    return _format_byte_buffer_segment_list(type_name, node, start_offset, total_len)


class ByteBufferViewPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_byte_buffer_test.cpp

    def __init__(self, val):
        self.__val = val

    def to_string(self):
        return _format_byte_buffer_view('byte_buffer_view', self.__val)

    def display_hint(self):
        return None


_ocudu_printers.append(PrinterSetup('byte buffer view', '^ocudu::byte_buffer_view$', ByteBufferViewPrinter))


class ByteBufferSlicePrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_byte_buffer_test.cpp

    def __init__(self, val):
        self.__val = val

    def to_string(self):
        return _format_byte_buffer_view('byte_buffer_slice', self.__val['sliced_view'])

    def display_hint(self):
        return None


_ocudu_printers.append(PrinterSetup('byte buffer slice', '^ocudu::byte_buffer_slice$', ByteBufferSlicePrinter))


######  span<T> ######

class SpanPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_span_test.cpp

    def __init__(self, val):
        self.__val = val

    def children(self):
        ptr = self.__val['ptr']
        length = int(self.__val['len'])
        for i in range(length):
            yield f'[{i}]', ptr[i]

    def to_string(self):
        length = int(self.__val['len'])
        return f'span of length {length}'

    def display_hint(self):
        return 'array'

_ocudu_printers.append(PrinterSetup('span', '^ocudu::span<.*>$', SpanPrinter))


######  Strong Type (strong_type<T, ...>) and derived types ######

class StrongTypePrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_strong_type_test.cpp

    def __init__(self, val):
        self.__val = val

    def to_string(self):
        val = self.__val['val']
        if val.type.strip_typedefs().code == gdb.TYPE_CODE_INT and val.type.strip_typedefs().sizeof == 1:
            # Cast to int for uint8_t/int8_t, to avoid that gdb prints a char.
            val = val.cast(gdb.lookup_type('int'))
        return f'{val}'

    def display_hint(self):
        return None


class StrongTypeLookup:
    # Pretty-printer lookup that matches any type inheriting from ocudu::strong_type<...>.

    def __init__(self):
        self.name = 'strong_type_lookup'
        self.enabled = True
        self.subprinters = []

    def __call__(self, val):
        if self._has_strong_type_base(val.type):
            return StrongTypePrinter(val)
        return None

    @staticmethod
    def _has_strong_type_base(ty):
        # Check if a type is or inherits from ocudu::strong_type<...>.
        type_name = str(ty.strip_typedefs())
        if type_name.startswith('ocudu::strong_type<'):
            return True
        try:
            for field in ty.strip_typedefs().fields():
                if field.is_base_class and StrongTypeLookup._has_strong_type_base(field.type):
                    return True
        except:
            pass
        return False


###### Log Likelihood Ratio (log_likelihood_ratio) ######

class LogLikelihoodRatioPrinter(printer_base):
    # Test: tests/utils/gdb/pretty_printers/pretty_printer_llr_test.cpp

    def __init__(self, val):
        self.__val = val

    def to_string(self):
        value_int8 = self.__val['value']
        intval = value_int8.cast(gdb.lookup_type('int'))
        return f'{intval}'

    def display_hint(self):
        return None


_ocudu_printers.append(PrinterSetup('llr', 'ocudu::log_likelihood_ratio', LogLikelihoodRatioPrinter))


###########################################################
# Set-up functions
###########################################################

def register_printers(objfile):
    global _ocudu_printers

    if objfile is None:
        objfile = gdb

    # Register printers with the new gdb.printing protocol, as subprinters of the OCUDU project printer.
    pp = gdb.printing.RegexpCollectionPrettyPrinter("ocudu_printer")
    [pp.add_printer(x.name, x.regex, x.printer) for x in _ocudu_printers]

    # Register the strong_type lookup first so it has lower priority than the regex collection.
    # GDB searches printers in reverse registration order, so the regex collection (registered
    # last) takes precedence for types like bf16_t that have a more specific printer.
    gdb.printing.register_pretty_printer(objfile, StrongTypeLookup())

    gdb.printing.register_pretty_printer(objfile, pp)
