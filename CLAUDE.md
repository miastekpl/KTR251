# CLAUDE.md - AI Assistant Guide for KTR251

> This document provides context and guidelines for AI assistants working with this repository.

## Project Overview

**Repository:** KTR251
**Status:** Initial setup / New project
**Last Updated:** 2026-02-04

This repository is currently in its initial setup phase. It serves as a scaffold for future development.

## Repository Structure

```
KTR251/
├── README.md          # Project description (minimal)
├── CLAUDE.md          # AI assistant guidelines (this file)
└── .git/              # Git repository metadata
```

### Current State

- **Source Code:** None yet - repository is empty
- **Dependencies:** None configured
- **Build System:** None configured
- **Tests:** None configured
- **CI/CD:** None configured

## Development Guidelines

### Git Workflow

**Branch Naming Convention:**
- Feature branches: `feature/<description>`
- Bug fixes: `fix/<description>`
- Claude AI branches: `claude/<session-identifier>`

**Commit Message Format:**
```
<type>: <short description>

[optional body with more details]
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

**Example:**
```
feat: Add user authentication module

- Implement JWT token handling
- Add login/logout endpoints
- Create user session management
```

### Code Conventions

When adding code to this repository, follow these conventions:

1. **File Organization:**
   - Group related files in logical directories
   - Use lowercase with hyphens for directory names (e.g., `src/user-auth/`)
   - Keep configuration files in the project root

2. **Naming:**
   - Use descriptive, meaningful names
   - Follow language-specific conventions (camelCase for JS, snake_case for Python, etc.)

3. **Documentation:**
   - Add inline comments for complex logic
   - Update README.md with setup instructions when adding dependencies
   - Document public APIs and interfaces

4. **Code Quality:**
   - Write clean, readable code
   - Avoid overly complex abstractions
   - Follow DRY (Don't Repeat Yourself) principles
   - Keep functions focused and single-purpose

## AI Assistant Instructions

### Before Making Changes

1. **Read existing code** before modifying - understand the context
2. **Check for existing patterns** - maintain consistency with established conventions
3. **Review related files** - understand dependencies and relationships

### When Writing Code

1. **Keep changes focused** - only modify what's necessary for the task
2. **Avoid over-engineering** - simple solutions are preferred
3. **Don't add unnecessary features** - stick to what's requested
4. **Security first** - never introduce vulnerabilities (XSS, injection, etc.)
5. **Test your changes** - verify code works before committing

### Git Operations

1. **Always commit with descriptive messages**
2. **Push to the designated branch** - check the branch name before pushing
3. **Don't force push** unless explicitly requested
4. **Keep commits atomic** - one logical change per commit

### What NOT to Do

- Don't create unnecessary documentation files
- Don't add unused dependencies
- Don't refactor code that isn't related to the current task
- Don't add comments stating the obvious
- Don't leave TODO comments without context
- Don't commit sensitive data (API keys, passwords, .env files)

## Build & Run

*To be updated when build system is configured.*

```bash
# Placeholder - update when applicable
# npm install / pip install / etc.
# npm run dev / python main.py / etc.
```

## Testing

*To be updated when testing framework is configured.*

```bash
# Placeholder - update when applicable
# npm test / pytest / etc.
```

## Dependencies

*To be updated when dependencies are added.*

| Dependency | Version | Purpose |
|------------|---------|---------|
| -          | -       | -       |

## Project-Specific Notes

*Add project-specific context, gotchas, or important notes here as the project evolves.*

---

## Maintenance

This file should be updated when:
- New major features or modules are added
- Build/test configurations change
- Development workflows are modified
- New conventions are established

**Last reviewed:** 2026-02-04
