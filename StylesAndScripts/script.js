function toggle(identity) { /*toggle based on the tag in the div of corresponding element*/
 var ele = document.getElementById(identity);
 var text = document.getElementById("displayText");
 if (ele.style.display == "block") {
  ele.style.display = "none";
  text.innerHTML = "+";
 }
 else if(ele.style.display == "none") {
  ele.style.display = "block";
  text.innerHTML = "-";
 }
}