find . -maxdepth 1 -mindepth 1 -type d | grep -v -E 'codec|lib' | xargs rm -rf

find . -maxdepth 1 -mindepth 1 -type d | grep -v -E 'codec|lib' | xargs rm -rf
find ./codec -maxdepth 1 -mindepth 1 | grep -v -E 'lib' | xargs rm -rf
find ./codec/lib/sm32 -maxdepth 1 -mindepth 1 | grep -v -E 'sbc.hex' | xargs rm -rf
