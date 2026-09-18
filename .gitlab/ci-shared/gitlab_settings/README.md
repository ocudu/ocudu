# GitLab Project Configuration with Terraform

This Terraform module manages the complete configuration of a GitLab project, including project settings, pipeline
schedules, protected branches, approval rules and more.

## Using it in Gitlab CI

This module is designed to work with the GitLab CI Terraform integration. The CI pipeline automatically:

- Validates Terraform configuration
- Plans changes and shows diff
- Applies changes
- Stores state in GitLab Terraform state backend

### Setup for New Projects

- CI Variable `GITLAB_TOKEN` needs to be created - see `Security Requirements` section
- Create the file `.gitlab/main.tf` and populate the values (you can check the file in this repo). Commit it (MR and/or
  main branch)

## Alternative: Manual Usage

1. Create and write down a new `main.tf` file.
2. Set up the required GitLab token (see Security Requirements below)
3. Set up environmental variables `TF_VAR_CI_PROJECT_ID`, `TF_VAR_CI_PROJECT_TITLE`. `TF_OCUDU_PATH`, `TF_OCUDU_REF`,
   `GITLAB_TOKEN` and `GITLAB_BASE_URL` (in case not using gitlab.com)
4. Run Terraform:

```bash
terraform init
terraform plan
terraform apply
```

## Security Requirements: GitLab Access Token Setup

This module requires a GitLab Personal Access Token or Project Access Token with full API permissions.

### Creating the Token

Create a `Personal Access Token` or a `Project Access Token` (Maintainer role). Select following scopes:

| Scope              | Purpose                                           | Required |
| ------------------ | ------------------------------------------------- | -------- |
| `api`              | Full GitLab API access for project management     | ✅ Yes   |
| `read_user`        | Read user information for provider authentication | ✅ Yes   |
| `read_repository`  | Access repository information                     | ✅ Yes   |
| `write_repository` | Modify repository settings                        | ✅ Yes   |

### Configuring in CI/CD

1. Go to **Project Settings > CI/CD > Variables**
2. Add variable `GITLAB_TOKEN`
3. **Security settings:**
   - ❌ **Protected** - We need to access it from branches to run the plan
   - ✅ **Masked** or **Masked and hidden** - Hidden in job logs
4. **Value**: Your generated token
