import dtl
import pyarrow as pa


def main():
    src = """
    WITH input AS IMPORT 'input';
    WITH output AS SELECT a, b FROM input WHERE a < b;
    EXPORT output TO 'output';
    """
    inputs = {"input": pa.table({"a": [1, 2, 3, 4, 5, 6], "b": [6, 5, 4, 3, 2, 1]})}
    outputs, trace = dtl.run(src, inputs=inputs)

    assert outputs["output"] == pa.table(
        {"a": [1, 2, 3], "b": [6, 5, 4]}
    )


if __name__ == "__main__":
    main()
