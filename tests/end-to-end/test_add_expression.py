import dtl
import pyarrow as pa


def main():
    src = """
    WITH input AS IMPORT 'input';
    WITH output AS SELECT a, b, a + b AS c FROM input;
    EXPORT output TO 'output';
    """
    inputs = {"input": pa.table({"a": [1, 2, 3, 4], "b": [3, 4, 5, 6]})}
    outputs, trace = dtl.run(src, inputs=inputs)
    assert outputs["output"] == pa.table(
        {"a": [1, 2, 3, 4], "b": [3, 4, 5, 6], "c": [4, 6, 8, 10]}
    )


if __name__ == "__main__":
    main()
