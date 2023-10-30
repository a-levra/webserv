import unittest
import subprocess
import time
import inspect

CONFIG_FILE = "test/conf/comments.conf"

BENCHMARK_TIME_SECONDS = 30
MINIMUM_AVAILABILITY_RATE = 95


class TestBenchmark(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.server_process = subprocess.Popen(
            ["./webserv", CONFIG_FILE],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL)
        time.sleep(2)

    @classmethod
    def tearDownClass(cls):
        cls.server_process.terminate()
        cls.server_process.wait()

    @staticmethod
    def parse_availability(siege_content):
        lines = siege_content.split('\n')
        availability_line = next(
            (line for line in lines if "availability" in line.lower()), None)
        if availability_line is None:
            return None
        availability = availability_line.split(":")[-1].strip(",%").strip()
        return float(availability)

    def test_single_page(self):
        siege_process = subprocess.Popen(
            ["siege", "-b", f"-t{BENCHMARK_TIME_SECONDS}s",
             "http://127.0.0.1:9000/"],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = siege_process.communicate()
        siege_process.wait()
        self.assertEqual(siege_process.returncode, 0)
        availability = self.parse_availability(stdout.decode())
        if availability is None:
            availability = self.parse_availability(stderr.decode())
        current_method_name = inspect.currentframe().f_code.co_name
        print(f"{current_method_name} availability: {availability}%",
              flush=True)
        if availability is None:
            self.fail("Fail parse availability")
        self.assertGreaterEqual(availability, MINIMUM_AVAILABILITY_RATE)


if __name__ == "__main__":
    unittest.main()
