import os
import pathlib
import subprocess
import tempfile

import pyarrow.parquet as pq

_DTL = os.environ['DTL']


def run(source, /, *, inputs):
    with tempfile.TemporaryDirectory() as tempdir:
        root_path = pathlib.Path(tempdir)

        source_path = root_path / "script.dtl"
        source_path.write_text(source)

        input_path = root_path / "input"
        input_path.mkdir()
        for input_name, input_table in inputs.items():
            pq.write_table(input_table, input_path / f"{input_name}.parquet")

        output_path = root_path / "output"
        output_path.mkdir()

        trace_path = root_path / "trace"
        trace_path.mkdir()

        subprocess.run(
            [_DTL, source_path, input_path, output_path, trace_path]
        ).check_returncode()

        outputs = {}
        for output_table_path in output_path.glob('*.parquet'):
            outputs[output_table_path.stem] = pq.read_table(output_table_path)

        return outputs, None
