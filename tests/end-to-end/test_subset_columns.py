import pyarrow as pa

import dtl


def main():
    src = """
    WITH input AS IMPORT 'input';
    WITH output AS SELECT a, c FROM input;
    EXPORT output TO 'output';
    """
    inputs = {
        "input": pa.table({"a": [1, 2, 3, 4], "b": [5, 6, 7, 8], "c": [9, 10, 11, 12]})
    }
    outputs, trace = dtl.run(src, inputs=inputs)
    assert outputs["output"] == pa.table({"a": [1, 2, 3, 4], "c": [9, 10, 11, 12]})


if __name__ == "__main__":
    main()
