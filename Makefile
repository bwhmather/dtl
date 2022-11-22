default: lint test

setup:
	meson setup build --warnlevel=3 --werror --debug

test:
	meson test -C build

lint:
	clang-format --dry-run -Werror src/*.?pp tests/*.?pp tests/*/*.?pp

format:
	clang-format -i src/*.?pp tests/*.?pp tests/*/*.?pp
