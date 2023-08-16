
/*
//Javascript from first script block in html
function returnBoxAction() {
}


//Javascript from second script block in html
var dropArea = document.getElementById('dropArea');

dropArea.addEventListener('dragenter', preventDefault, false);
dropArea.addEventListener('dragover', preventDefault, false);
dropArea.addEventListener('drop', preventDefault, false);

dropArea.addEventListener('drop', handleDrop, false);

function preventDefault(event) {
  event.preventDefault();
  event.stopPropagation();
}

function handleDrop(event) {
  var files = event.dataTransfer.files;
  console.log(files);
}

*/
//Might be wrong with .submit_button

function onEncodePressed() {
  alert("Encode pressed!");
  var formData = new FormData();
  formData.append('action', 'upload_encode');
  formData.append('file', $('inputForm')[0]); 
  $.ajax({
    url: 'http://localhost/website/Handle.php',
    type: 'POST',
    data: formData, // your form data or any other data to be sent
    processData: false,
    contentType: false,
    success: function(response) {
      // Handle the response and display an alert or perform other actions
      alert(response);
    },
    error: function() {
      alert('An error occurred');
    }
  });
}

function onDecodePressed(){
  alert("Decode pressed!");
  $.ajax({
    url: 'http://localhost/website/Handle.php',
    type: 'POST',
    data: new FormData($('inputForm')[0]),
    enctype: 'multipart/form-data',
    action: 'upload_decode',

    //Must include these options
    cache: false,
    contentType: false,
    processData: false,
  }).done(decodeFinish());
}

function encodeFinish() {
  alert("Encode finished!");
}

function decodeFinish() {
  //Call on ajax return for decode finish
  //link -- backend/output_decode.txt
}
