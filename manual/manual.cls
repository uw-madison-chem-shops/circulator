\ProvidesClass{manual}

\LoadClass{article}
\RequirePackage[letterpaper, margin=1in]{geometry} % 1 inch margins required

% text
\RequirePackage[utf8]{inputenc}
\setlength\parindent{0pt}
\setlength{\parskip}{1em}
\renewcommand{\familydefault}{\sfdefault}
\RequirePackage{setspace}

% force all floats to center (see https://tex.stackexchange.com/a/53383)
\makeatletter
\g@addto@macro\@floatboxreset{\centering}
\makeatother

% each section is a new page
\let\stdsection\section
\renewcommand\section{\clearpage\stdsection}

% hyperref
\RequirePackage[colorlinks=true, linkcolor=black, urlcolor=blue, citecolor=black,
anchorcolor=black]{hyperref}

% images
\RequirePackage{graphicx}
\RequirePackage{pdfpages}


\usepackage{array}
\newcolumntype{P}[1]{>{\raggedleft\arraybackslash}p{#1}}

\RequirePackage[shortlabels]{enumitem}
\setlist[itemize, 1]{nosep}
\setlist[itemize, 2]{nosep, topsep=-5ex}
\setlist[itemize, 3]{nosep, topsep=-5ex}
\setlist[itemize, 4]{nosep, topsep=-5ex}
\newenvironment{ditemize}
  {
  \begin{itemize}
  \renewcommand{\labelitemi}{$\rightarrow$}
  \singlespacing
  }
  {
  \end{itemize}
  }
