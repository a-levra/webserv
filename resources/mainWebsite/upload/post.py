import cgi
import os

method = os.environ.get("REQUEST_METHOD")

if method == "POST":
    form = cgi.FieldStorage()
    upload_file_begin = ""
    upload_file_end = ""
    message = ""

    with open('upload-fileBEGIN.html', 'r') as fichier:
        upload_file_begin = fichier.read()
    if "file" in form:
        file_item = form["file"]
        if file_item.file:
            if not os.path.exists("files/"):
                os.mkdir("files/")
            try:
                file_path = 'files/' + os.path.basename(file_item.filename)
                with open(file_path, 'wb') as f:
                    f.write(file_item.file.read())
                message = "Le fichier a été téléchargé avec succès"
            except:
                message = "Erreur: le fichier n'a pas pu etre enregistré"

        else:
            message = "Erreur : Le fichier est vide ou n'a pas été téléchargé correctement"
    else:
        message = "Erreur : Aucun fichier n'a été soumis"

    upload_file_begin += f"<p id='error'> {message} </p>"
    print(upload_file_begin)
    directory = "files/"
    files = os.listdir(directory)
    for file in files:
        print(f'<a href="files/{file}"><div>{file}</h5></div>')

    with open('upload-fileEND.html', 'r') as fichier:
        upload_file_end = fichier.read()
    print(upload_file_end)
else:
    print("Accept only post request")
