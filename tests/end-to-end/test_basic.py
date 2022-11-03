import dtl
import pyarrow as pa

def main():
    src = """
    WITH input AS IMPORT 'input';
    EXPORT input TO 'output';
    """
    inputs = {"input": pa.table({"column": [1, 2, 3, 4]})}
    outputs, trace = dtl.run(src, inputs=inputs)
    assert outputs["output"] == pa.table({"column": [1, 2, 3, 4]})

if __name__ == '__main__':
    main()
