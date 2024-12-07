
   function toggleDisplay(stuffToToggle,elementThatExpandsStuff,elementThatCollapsesStuff) {
      if ( stuffToToggle.style.display == 'none' ) {
         stuffToToggle.style.display = '';
         elementThatExpandsStuff.style.display = 'none';
         elementThatCollapsesStuff.style.display = '';
      }
      else {
         stuffToToggle.style.display = 'none';
         elementThatExpandsStuff.style.display = '';
         elementThatCollapsesStuff.style.display = 'none';
      }
   }
