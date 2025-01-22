import dtl
import pyarrow as pa


def main():
    src = """
    WITH a AS IMPORT 'a';
    WITH b AS IMPORT 'b';
    WITH output AS SELECT x, y FROM a JOIN b;
    EXPORT output TO 'output';
    """
    inputs = {
        "a": pa.table({"x": [1, 2, 3, 4]}),
        "b": pa.table({"y": [3, 4, 5]}),
    }
    outputs, trace = dtl.run(src, inputs=inputs)
    assert outputs["output"] == pa.table({
        "x": [1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4],
        "y": [3, 4, 5, 3, 4, 5, 3, 4, 5, 3, 4, 5]
    })


if __name__ == "__main__":
    main()
