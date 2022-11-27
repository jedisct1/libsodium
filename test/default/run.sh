#! /bin/sh

find . -type f -perm -100 -print | grep -v run.sh | sort | while read -r x; do
  echo "[$x]"
  if ! "$x"; then
    echo "*** [$x] FAILED" >&2
    exit 1
  fi
done
