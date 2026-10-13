This directory contains files encrypted with turbocrypt for project
maintainers only.

The encrypted files and their names are stored in Git so maintainers can
share them without exposing their contents.

If you're checking out this project without a key, you can leave this
directory alone.

You don't need to decrypt these files to work with the public files.

Get turbocrypt: https://github.com/jedisct1/turbocrypt

Maintainers with the appropriate key can restore their private files by
running this command from the repository:

    turbocrypt git unlock --key /path/to/key

Edit the restored files in the working tree. Let turbocrypt manage the
encrypted copies in this directory.
