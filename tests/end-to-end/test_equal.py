import dtl
import pyarrow as pa


def main():
    src = """
    WITH input AS IMPORT 'input';
    WITH output AS SELECT * FROM input WHERE a = 3;
    EXPORT output TO 'output';
    """
    inputs = {"input": pa.table({"a": [1, 2, 3, 4, 3], "b": [3, 4, 5, 6, 7]})}
    outputs, trace = dtl.run(src, inputs=inputs)
    assert outputs["output"] == pa.table(
        {"a": [3, 3], "b": [5, 7]}
    )


if __name__ == "__main__":
    #    main()
    pass
