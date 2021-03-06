if exists('s:loaded')
  finish
endif
let s:loaded = 1

let s:path = expand('<sfile>:p:h')

lcd <sfile>:h

function s:build_now() abort
  compiler gcc
  execute 'AsyncRun -cwd=<root> build.bat'
  copen
endfunction

noremap <F3> :AsyncRun -cwd=<root> ctags -R --sort=foldcase --extra=+fq --fields=+ianS --exclude=Crosstools --exclude=build<CR>
noremap <silent> <F5> :call <SID>build_now()<CR>
noremap <F6> :AsyncRun -mode=term -cwd=<root> -pos=thelp build.bat<CR>
noremap <F10> :call asyncrun#quickfix_toggle(8)<CR>

if !has('win32')
  let g:gutentags_ctags_exclude = ['toolchain']

  " let g:completor_debug = 1
  let g:completor_clang_binary = '/usr/bin/clang'

  function! MyNeomakeClang(jobinfo) dict abort
    let maker = deepcopy(self)
    if self.name == 'clang' && filereadable('.clang')
      let maker.args += split(join(readfile('.clang'), "\n"))
    endif
    return maker
  endfunction
  call neomake#config#set('ft.c.InitForJob', function('MyNeomakeClang'))
endif
