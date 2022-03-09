# crealtime
Run C files with simple shebang


## Help
```bash
Usage: crealtime [flags] <file-path> [{ arg1 arg2 ...}]
Example: crealtime -f /bin/join_strings { , hello world }

Parameters:
    <file-path> - string; event-source path

Flags:
    -f - force recompilation
    -c - compile the source but not run it

Env variables:
    CREALTIME_BIN_PATH - string; a path, where temporary binaries will reside
```

## The shebang
```bash
//usr/local/bin/crealtime "$0" { $@ }; exit
```

## Test usage (or just an example)
1. `make install` crealtime
2. Just run `test/main`
3. You can also pass some args in there: `test/main a b c "test test"`

