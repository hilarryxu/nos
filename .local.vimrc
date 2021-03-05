if exists('s:loaded')
  finish
endif
let s:loaded = 1

let s:path = expand('<sfile>:p:h')

lcd <sfile>:h

noremap <F6> :AsyncRun -mode=term -cwd=<root> -pos=thelp build.bat<CR>
