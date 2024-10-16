import pyarrow as pa

import dtl


def main():
    src = """
    WITH input AS IMPORT 'input';
    EXPORT SELECT a FROM input TO 'a';
    EXPORT SELECT b FROM input TO 'b';
    EXPORT SELECT c FROM input TO 'c';
    """
    inputs = {
        "input": pa.table({"a": [1, 2, 3, 4], "b": [5, 6, 7, 8], "c": [9, 10, 11, 12]})
    }
    outputs, trace = dtl.run(src, inputs=inputs)

    expected = {
        "a": pa.table({"a": [1, 2, 3, 4]}),
        "b": pa.table({"b": [5, 6, 7, 8]}),
        "c": pa.table({"c": [9, 10, 11, 12]}),
    }
    assert outputs == expected


if __name__ == "__main__":
    main()
