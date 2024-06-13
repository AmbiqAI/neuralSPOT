# Concepts
The ini file specifies which files contain tests, which tests to run in those tests, and which tests can be run sequentially without a reset (which is effectively a recompile).

```ini
[test file name]
test_list = foo boo choo

[test file name] # can be different or same as above
test_list = a b c
```

There will be ini files for smoke, full regression, slow tests, etc.
