from pydrive.drive import GoogleDrive 
from pydrive.auth import GoogleAuth 
from pydrive.files import GoogleDriveFileList

from apiclient.http import MediaFileUpload
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
import googleapiclient.errors
import os.path
import pickle
import ast
import sys
import os 

ROOT_ID = "1cNvSwxqmy29VNE9WnB5qAhiyPmQfaaIy"

def get_folder_id(drive, parent_folder_id, folder_name):
    """ 
		Check if destination folder exists and return it's ID
	"""

    # Auto-iterate through all files in the parent folder.
    file_list = GoogleDriveFileList()
    try:
        file_list = drive.ListFile(
			{'includeTeamDriveItems': "true", 'supportsTeamDrives': "true", 'q': "'{0}' in parents and trashed=false".format(parent_folder_id)}
		).GetList()
	# Exit if the parent folder doesn't exist
    except googleapiclient.errors.HttpError as err:
		# Parse error message
        message = ast.literal_eval(err.content)['error']['message']
        if message == 'File not found: ':
            print(message + folder_name)
            exit(1)
		# Exit with stacktrace in case of other error
        else:
            raise

	# Find the the destination folder in the parent folder's files
    for file1 in file_list:
        if file1['title'] == folder_name:
            #print('title: %s, id: %s' % (file1['title'], file1['id']))
            return file1['id']

def create_folder(drive, folder_name, parent_folder_id):
    """ 
		Create folder on Google Drive
	"""
    folder_metadata = {'title': folder_name, 'mimeType': 'application/vnd.google-apps.folder', 'parents': [{"kind": "drive#fileLink", "id": parent_folder_id}]}
    folder = drive.CreateFile(folder_metadata)
    folder.Upload(param={'supportsTeamDrives': True})
    #print('title: %s, id: %s' % (folder['title'], folder['id']))

def validate_and_create_folder(parent_folder_id, folderName):
    file_list = drive.ListFile({'includeTeamDriveItems': "true", 'supportsTeamDrives': "true", 'q': "'{0}' in parents and trashed=false".format(parent_folder_id)}).GetList()
    folder_list = {}
    for file1 in file_list:
        #print ('title: %s, id: %s' % (file1['title'], file1['id']))
        folder_list[file1["title"]] = file1["id"]

    if (folderName not in folder_list):
        create_folder(drive, folderName, parent_folder_id)
    else:   
        print("The folder is already there")
    return get_folder_id(drive, parent_folder_id, folderName)
    
if __name__ == "__main__":
    passTime = sys.argv[1].split("/")
    year = passTime[0]
    month = passTime[1]
    currentDay = "Day " + passTime[2]
    monthOfYear = year + month
    gauth = GoogleAuth() 
    
    # Creates local webserver and auto 
    # handles authentication. 
    gauth.LoadCredentialsFile("GoogleDriveCredentials.txt")
    if gauth.credentials is None:
        # Authenticate if they're not there
        gauth.LocalWebserverAuth()
    elif gauth.access_token_expired:
        # Refresh them if expired
        print("Google Drive Token Expired, Refreshing")
        gauth.Refresh()
    else:
        # Initialize the saved creds
        gauth.Authorize()
    # Save the current credentials to a file
    gauth.SaveCredentialsFile("GoogleDriveCredentials.txt")
    # Initiate drive instance
    drive = GoogleDrive(gauth) 

    monthFolderID = validate_and_create_folder(ROOT_ID, monthOfYear)
    dayFolderID = validate_and_create_folder(monthFolderID, currentDay)
    # replace the value of this variable 
    # with the absolute path of the directory 
    path = "/home/binhnt174/NEXT/image"  


    # iterating thought all the files/folder 
    # of the desired directory 
    
    for x in os.listdir(path): 
        f = drive.CreateFile({'title': x, 'parents': [{'id': '{0}'.format(dayFolderID), 'kind': 'drive#fileLink'}], 'mimeType' : 'image/jpg', }, )

        # include only image file
        if ("png" in x or "jpg" in x):
            f.SetContentFile(os.path.join(path, x)) 
            f.Upload(param={'supportsTeamDrives': True})
        os.remove(os.path.join(path, x))

        # Due to a known bug in pydrive if we  
        # don't empty the variable used to 
        # upload the files to Google Drive the 
        # file stays open in memory and causes a 
        # memory leak, therefore preventing its  
        # deletion 
        f = None
    
