import dtl
import pyarrow as pa


def main():
    src = """
    WITH a AS IMPORT 'a';
    WITH b AS IMPORT 'b';
    WITH output AS SELECT id, x, y FROM a JOIN b ON id = aid;
    EXPORT output TO 'output';
    """
    inputs = {
        "a": pa.table({
            "id": [1, 2, 3, 4],
            "x": [2, 4, 6, 8],
        }),
        "b": pa.table({
            "aid": [2, 2, 4],
            "y": [3, 4, 5],
        }),
    }
    outputs, trace = dtl.run(src, inputs=inputs)

    assert outputs["output"] == pa.table({
        "id": [2, 2, 4],
        "x": [4, 4, 8],
        "y": [3, 4, 5],
    })


if __name__ == "__main__":
    main()
