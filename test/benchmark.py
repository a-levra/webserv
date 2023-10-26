import unittest
import subprocess
import time
import signal

CONFIG_FILE = "test/conf/comments.conf"


class TestBenchmark(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        print("start serv", flush=True)
        cls.server_process = subprocess.Popen(
            ["./webserv", CONFIG_FILE], stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL)
        # print("communicate serv", flush=True)
        # cls.stdout, cls.stderr = cls.server_process.communicate()
        time.sleep(2)

    @classmethod
    def tearDownClass(cls):
        print("terminate serv", flush=True)
        cls.server_process.terminate()
        print("terminate serv", flush=True)
        cls.server_process.send_signal(signal.SIGINT)
        print("terminate serv", flush=True)
        cls.server_process.wait()
        print("server is down", flush=True)
        # print(f"Server out: {cls.stdout}")

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
        print("start test", flush=True)
        siege_process = subprocess.Popen(
            ["siege", "-b", "-t30s", "http://127.0.0.1:9000/"],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = siege_process.communicate()
        print("Wait siege", flush=True)
        siege_process.wait()
        print("Finish wait siege", flush=True)
        self.assertEqual(siege_process.returncode, 0)
        print(stdout)
        print(stderr)
        availability = self.parse_availability(stdout.decode())
        print(f"Availability: {availability}", flush=True)
        if availability is None:
            self.fail("Fail parse availability")
        self.assertGreaterEqual(availability, 95)


if __name__ == "__main__":
    unittest.main()
