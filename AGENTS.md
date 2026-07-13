## C++ guidelines

### Configuration Types

- prefer strong-types over raw integers for config parameters that represent time. Use chrono types like std::chrono::milliseconds by default, or custom enum classes if the user requests it.


