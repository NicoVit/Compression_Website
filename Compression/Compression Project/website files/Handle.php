<?php
    if (isset($_POST['message'])){
        switch($_POST['message']){
            case 'upload_encode':
                upload_encode($_POST['name']);
            case 'upload_decode':
                upload_decode($_POST['name']);
        }
    }

    function upload_encode($name){
      //Upload file to server
      /*
      ./encode -i <file name> -o <file name>
      ./decode -i <file name> -o <file name>
      */
      
      $file_upload_destination = 'backend/input_encode.txt';
      $file_download_destination = 'backend/output_encode.txt';

      $file = $_FILES['file'];

      $fileTemporaryName = $_FILES['file']['tmp_name'];
      $error = $_FILES['file']['error'];
      if ($error != 0){
        return;
      } else {
        move_uploaded_file($fileTemporaryName, $file_upload_destination);
      }
      $command='.backend/encode -i input_encode.txt -o output_encode.txt';
      $output=null;
      $retval=null;
      exec($command, $output, $retval);
    
     // return "Upload and encoding completed";
    }


    function upload_decode($name){
        //Upload file to server
        /*
        ./encode -i <file name> -o <file name>
        ./decode -i <file name> -o <file name>
        */

        $file_upload_destination = 'backend/input_decode.txt';
        $file_download_destination = 'backend/output_decode.txt';

        $file = $_FILES['file'];

        $fileTemporaryName = $_FILES['file']['tmp_name'];
        $error = $_FILES['file']['error'];
        if ($error != 0){
          return;
        } else {
          move_uploaded_file($fileTemporaryName, $file_upload_destination);
        }

        $command='.backend/decode -i input_decode.txt -o output_decode.txt';
        $output=null;
        $retval=null;
        exec($command, $output, $retval);
    }
?>