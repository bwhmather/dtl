import pyarrow as pa

import dtl


def main():
    src = """
    WITH input AS IMPORT 'input';
    WITH output AS SELECT old_column AS new_column FROM input;
    EXPORT output TO 'output';
    """
    inputs = {"input": pa.table({"old_column": [1, 2, 3, 4]})}
    outputs, trace = dtl.run(src, inputs=inputs)
    assert outputs["output"] == pa.table({"new_column": [1, 2, 3, 4]})


if __name__ == "__main__":
    main()
