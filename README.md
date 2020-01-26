Application based on CloudMirror sample: https://github.com/Microsoft/Windows-classic-samples/tree/master/Samples/CloudMirror

This Application is created to reproduce issues with Cloud Filter and Foobar2000

How to reproduce error:

1. Open project in Visual Studio 2019 and set one of project as default (I don't think it matters which one you choose)
2. Download foobar2000 from Windows Store: https://www.microsoft.com/pl-pl/p/foobar2000/9pdj8x9spf2k
3. Download https://www.bensound.com/royalty-free-music/track/ukulele and place it in C:/ drive, so file "C:/bensound-ukulele.mp3" exists
4. Start application and select directory
5. Open this directory and open test.mp3 in Groove music player
6. Verify that seeking works fine
7. Ctrl-C in application
8. Remove test.mp3 in directory (if exist)
9. Repeat step 4
10. Open this directory and open test.mp3 in foobar2000
11. Seek throught music

You should have a lot of these errors

```
Decoding failure at 0:02.012 (I/O error (win32 #379)):
"C:\LOCATION\test.mp3"

Decoding failure at 0:29.066 (I/O error (win32 #379)):
"C:\LOCATION\test.mp3"

Decoding failure at 0:35.936 (I/O error (win32 #379)):
"C:\LOCATION\test.mp3"
```

Using Process Monitor it can be seen that error happens during `ReadFile` command and it is `0xC000CF0A` which means `STATUS_CLOUD_FILE_NOT_SUPPORTED` (The operation is not supported by the cloud sync provider.). Documentation does indeed mention this error, but it is returned to Cloud Sync software, not external client.