import cgi
import os
import uuid
import http.cookies

default_page = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Upload</title>
    <link rel="stylesheet" type="text/css" href="../css/style.css">
</head>
<body>
<div class="navbar">
    <a href="/">Home</a>
    <a href="/upload/">Upload</a>
    <a href="/delete/">Delete</a>
    <a href="/cookies/">Cookies</a>
    <a href="/google/">Google</a>
    <a href="/listing-directory/">Listing Directory</a>
    <a href="/curl/">Curl</a>
</div>
    <h2>Connexion</h2>
    <form action="cookies.py" method="post">
        <div>
            <label for="login">Nom d'utilisateur :</label>
            <input type="text" id="login" name="login" required>
        </div>

        <div>
            <label for="password">Mot de passe :</label>
            <input type="password" id="password" name="password" required>
        </div>
        <br>
        <div>
            <input type="submit" value="Se connecter">
        </div>
    </form>
"""

def get_login_page(login, password):
    return f"""
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Upload</title>
<link rel="stylesheet" type="text/css" href="../css/style.css">
<script>
function deleteAllCookiesAndRedirect(url) {{
    const cookies = document.cookie.split(";");

    for (let i = 0; i < cookies.length; i++) {{
        const cookie = cookies[i];
    const eqPos = cookie.indexOf("=");
    const name = eqPos > -1 ? cookie.substr(0, eqPos) : cookie;
        document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
    }}
    window.location.href = url;
}}
</script>
</head>
<body>
<div class="navbar">
<a href="/">Home</a>
<a href="/upload/">Upload</a>
<a href="/delete/">Delete</a>
<a href="/cookies/">Cookies</a>
</div>
<h2>You are logged in as:</h2>
<p>{login}</p>
<p>{password}</p>
<button id="disconnectButton" onclick="deleteAllCookiesAndRedirect('/cookies/')"> Se déconnecter</button>
"""



def get_session(session_id):
    files = os.listdir("sessions/")
    for file in files:
        if file == session_id:
            with open(f"sessions/{file}", "r") as session_file:
                r = session_file.readlines()
                if len(r) == 2:
                    return r[0], r[1]
    return None


def get_view():
    cookies = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))
    session_cookie = cookies.get("session_id")
    if session_cookie is None:
        print(default_page)
        return
    log = get_session(session_cookie.value)
    if log is None:
        print(default_page)
        return
    print(get_login_page(log[0], log[1]))


def post_view():
    form = cgi.FieldStorage()
    if "login" in form and "password" in form:
        login = form["login"].value
        password = form["password"].value
        session_id = str(uuid.uuid4())
        print(f"Set-Cookie: session_id={session_id}\r\n\r\n", end="")
        with open(f"sessions/{session_id}", "w") as file:
            file.writelines([f"{login}\n", f"{password}\n"])
        print(get_login_page(login, password))
    else:
        print("Invalid form")


method = os.environ.get("REQUEST_METHOD")
if not os.path.exists("sessions/"):
    os.mkdir("sessions/")
if method == "GET":
    get_view()
elif method == "POST":
    post_view()

