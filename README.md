# MpqSimpleIO

CLI to work with old MPQ files. Tested only on Diablo 2.

Based on the great [StormLib library](https://github.com/ladislav-zezula/StormLib).

## Usage

### List MPQ files

    MpqSimpleIO -l <MPQ path> [mask] [path to additional listfile]
    
    # example
    MpqSimpleIO -l /some/path/1.mpq

- If you don't pass mask, `*` is implied (to list all files)
- If you want to pass additional listfile, mask is required

### Extract files to disk

    MpqSimpleIO -r <MPQ path> <mask> <path to extract dir> [path to additional listfile]
    
    # example
    MpqSimpleIO -r /some/path/1.mpq 'data\global\excel\*.bin' /some/extract/path

The directory where you want to extract files must exist.

### Add files to MPQ

    MpqSimpleIO -w <MPQ path> --prefix=<internal MPQ path prefix> <files...>
    
    # example
    MpqSimpleIO -w /some/path/1.mpq --prefix='data\global\excel\' /path/to/file /path/to/another/file

You can also put multiple files under different prefixes:

    MpqSimpleIO -w /some/path/1.mpq /path/to/file 'prefix\for\first\file\' /path/to/another/file 'prefix\for\second\file\'
