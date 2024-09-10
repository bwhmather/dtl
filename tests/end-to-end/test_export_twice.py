import pyarrow as pa

import dtl


def main():
    src = """
    WITH table AS IMPORT 'input';
    EXPORT SELECT column AS a FROM table TO 'output';
    EXPORT SELECT column AS b FROM table TO 'output';
    """
    inputs = {"input": pa.table({"column": [1, 2, 3, 4]})}
    outputs, trace = dtl.run(src, inputs=inputs)
    assert outputs["output"] == pa.table({"b": [1, 2, 3, 4]})


if __name__ == "__main__":
    main()
