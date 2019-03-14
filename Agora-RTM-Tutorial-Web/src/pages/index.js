$(() => {
  const submit = (e) => {
    e.preventDefault();
    const accountName = $("#accountName").val();
    if(!accountName) {
      $("#accountNameRequired").css({
        "display": "block"
      });
      return;
    }
    $(location).attr('href', `/meeting?account=${accountName}`);
  }
  $("#form").on('submit', submit);
  $("#submit").on("keypress", submit);
})