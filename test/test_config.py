import os
import unittest
import subprocess

CONFIG_DIRECTORY = "test/conf/"


class TestConfigFile(unittest.TestCase):

    def test_empty_conf(self):
        server_process = subprocess.Popen(
            ["./webserv", CONFIG_DIRECTORY + "empty.conf"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL)
        try:
            server_process.wait(2)
        except subprocess.TimeoutExpired:
            server_process.terminate()
            server_process.wait()
            self.fail()
        self.assertEqual(server_process.returncode, 1)

    def test_with_comments(self):
        server_process = subprocess.Popen(
            ["./webserv", CONFIG_DIRECTORY + "comments.conf"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL)
        try:
            server_process.wait(2)
        except subprocess.TimeoutExpired:
            server_process.terminate()
            server_process.wait()
        self.assertEqual(server_process.returncode, 0)


if __name__ == '__main__':
    unittest.main()
