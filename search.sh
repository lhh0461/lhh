find ./ -name "*.[c|h]" -o -name "*.lua" | xargs grep -n "$@" --color=auto
