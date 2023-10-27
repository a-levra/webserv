import unittest
import requests
import subprocess
import time

CONFIG_FILE = "test/conf/comments.conf"


class TestGetRequest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.server_process = subprocess.Popen(
            ["./webserv", CONFIG_FILE],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        time.sleep(2)

    @classmethod
    def tearDownClass(cls):
        cls.server_process.terminate()
        cls.server_process.wait()

    def test_simple_get(self):
        response = requests.get("http://localhost:9000/")
        self.assertEqual(response.status_code, 404)

    def test_invalid_port(self):
        with self.assertRaises(requests.RequestException) as cm:
            response = requests.get("http://localhost:9090")


if __name__ == "__main__":
    unittest.main()
