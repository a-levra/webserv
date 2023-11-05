import os
import unittest
import subprocess

CONFIG_DIRECTORY = "test/conf/"


class TestConfigFile(unittest.TestCase):

    @staticmethod
    def run_server(config_file):
        server_process = subprocess.Popen(
            ["./webserv", config_file],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL)
        try:
            server_process.wait(2)
        except subprocess.TimeoutExpired:
            server_process.terminate()
            server_process.wait()
            return server_process.returncode
        return server_process.returncode

    def test_empty_conf(self):
        return_code = self.run_server(CONFIG_DIRECTORY + "empty.conf")
        self.assertEqual(return_code, 1)

    def test_with_comments(self):
        return_code = self.run_server(CONFIG_DIRECTORY + "comments.conf")
        self.assertEqual(return_code, 0)

    def test_invalid_ip(self):
        return_code = self.run_server(CONFIG_DIRECTORY + "invalid_ip.conf")
        self.assertEqual(return_code, 1)

    def test_invalid_port(self):
        return_code = self.run_server(CONFIG_DIRECTORY + "invalid_port.conf")
        self.assertEqual(return_code, 1)

    def test_no_server(self):
        return_code = self.run_server(CONFIG_DIRECTORY + "no_server.conf")
        self.assertEqual(return_code, 1)

    def test_forbidden_directive(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "forbidden_directive.conf")
        self.assertEqual(return_code, 1)

    def test_unclose_directive(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "unclose_directive.conf")
        self.assertEqual(return_code, 1)

    def test_unclose_context(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "unclose_context.conf")
        self.assertEqual(return_code, 1)

    def test_no_open_context(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "no_open_context.conf")
        self.assertEqual(return_code, 1)

    def test_conflict_location(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "conflict_location.conf")
        self.assertEqual(return_code, 1)

    def test_conflict_servername(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "conflict_servername.conf")
        self.assertEqual(return_code, 1)

    def test_location_without_arg(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "location_without_arg.conf")
        self.assertEqual(return_code, 1)

    def test_single_line(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "single_line.conf")
        self.assertEqual(return_code, 0)

    def test_no_http_context(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "no_http_context.conf")
        self.assertEqual(return_code, 1)

    def test_invalid_cgi_path(self):
        return_code = self.run_server(CONFIG_DIRECTORY +
                                      "invalid_cgi_path.conf")
        self.assertEqual(return_code, 1)


if __name__ == '__main__':
    unittest.main()
