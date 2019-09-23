jQuery(document).ready(function($) {

  //These vars are grabbed from response
  var firstN = 'jsFName'; 
  var lastN = 'jsLName';

//edit-school-select is the html tag for something
  $("#edit-school-select").change(function () {
    $.ajax({
      type: 'POST',
      url: 'http://localhost:8080/?q=ajax/test',
      contentType: "application/json",
      dataType: 'json',
      data: { first_name: firstN, last_name: lastN },
      success: ajaxSuccess,
      error: ajaxError,
      complete: ajaxComplete,
    });
  });



  function ajaxSuccess(data,response){
    //graphs will go here
    schoolBoxChart.render();
    schoolBarGraph.render();
    studentLineGraph.render();

  }

  function ajaxError(xml, error) {
    console.log(error);
  }

  function ajaxComplete(){
    console.log('Completed.');
  }


});
