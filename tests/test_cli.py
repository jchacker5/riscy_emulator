"""Exercise CLI contracts, including bounded execution and binary input."""
import pathlib
import subprocess
import sys
import tempfile
import unittest

BINARY = str(pathlib.Path(sys.argv.pop(1)).resolve())


class CliTests(unittest.TestCase):
    def run_cli(self, *args, data=b""):
        return subprocess.run([BINARY, *args], input=data, capture_output=True, timeout=5)

    def test_examples(self):
        result = self.run_cli()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout, b"Result at memory[0xFF]: 6\n")
        result = self.run_cli("--program", "fib", "-d")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn(b"memory[0xFF]: 8", result.stdout)
        self.assertIn(b"PC=00, inst=ff, op=SLI", result.stderr)

    def test_invalid_arguments(self):
        for args in [("--input", "256"), ("--input", "-1"), ("--input", "1x"),
                     ("--input", ""), ("--max-steps", "0"), ("--max-steps", "-1"),
                     ("--max-steps", "99999999999999999999999"), ("--program", "conway"),
                     ("--rom",), ("--bad", "value")]:
            with self.subTest(args=args):
                self.assertEqual(self.run_cli(*args).returncode, 2)

    def test_rom_failures(self):
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / "test.bin"
            for image, error in [(b"", b"1..256"), (b"\0" * 257, b"1..256"),
                                 (b"\0" * 256, b"instruction limit"),
                                 (b"\x90", b"invalid instruction"),
                                 (b"\x83", b"input exhausted")]:
                with self.subTest(error=error):
                    path.write_bytes(image)
                    result = self.run_cli("--rom", str(path), "--max-steps", "600")
                    self.assertEqual(result.returncode, 1)
                    self.assertIn(error, result.stderr)
            path.write_bytes(b"\x83\x82\x01")
            result = self.run_cli("--rom", str(path), data=b" ")
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue(result.stdout.startswith(b" Result at"), result.stdout)
            self.assertEqual(self.run_cli("--rom", str(path), "--input", "2").returncode, 2)

    def test_missing_file(self):
        self.assertEqual(self.run_cli("--rom", "/no/such/rom.bin").returncode, 1)


if __name__ == "__main__":
    unittest.main()
