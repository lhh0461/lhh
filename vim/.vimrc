set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()

Plugin 'VundleVim/Vundle.vim'

"plugin list
Plugin 'scrooloose/nerdtree'
Plugin 'kien/ctrlp.vim'
Plugin 'dholm/tabbar'
Plugin 'majutsushi/tagbar'
Plugin 'flazz/vim-colorschemes'
Plugin 'Lokaltog/vim-powerline'
Plugin 'grep.vim'
Plugin 'dyng/ctrlsf.vim' "need ag or ack
Plugin 'scrooloose/nerdcommenter' "批量注释
Plugin 'SirVer/ultisnips' "自动填充代码块
"Plugin 'Valloric/YouCompleteMe'
"Plugin 'rking/ag.vim'

"color 
"Plugin 'altercation/solarized'

call vundle#end()            " required
filetype plugin on

" :PluginList       - lists configured plugins
" :PluginInstall    - installs plugins; append `!` to update or just
" :PluginUpdate
" :PluginSearch foo - searches for foo; append `!` to refresh local cache
" :PluginClean      - confirms removal of unused plugins; append `!` to


nmap <F5> :!ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .<CR><CR> :TlistUpdate<CR>
imap <F5> <ESC>:!ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .<CR><CR> :TlistUpdate<CR>
set tags=tags
set tags+=./tags "add current directory's generated tags file 
"set tags+=/home/lhh/workspace/ldmud/tags "add new tags file(刚刚生成tags的路径，在ctags -R 生成tags文件后，不要将tags移动到别的目录，否则ctrl+］时，会提示找不到源码文件) 
"set tags+=/home/lhh/workspace/MudOS_v21/tags "add new tags file(刚刚生成tags的路径，在ctags -R 生成tags文件后，不要将tags移动到别的目录，否则ctrl+］时，会提示找不到源码文件) 
nmap <F6> <C-P>
nmap <F7> :NERDTreeToggle<CR>
imap <F7> <ESC>:NERDTreeToggle<CR>
nmap <F8> :TagbarToggle<CR>
imap <F8> <ESC>:TagbarToggle<CR>
nmap <C-X> :w<CR>
imap <C-X> <ESC>:w<CR>
"nmap <F9> :TlistToggle"<CR>
nmap <F9> <C-O>
nmap <F10> <C-]>


let mapleader=","

" nerdtree 
" vim打开文件时自动打开
" autocmd vimenter * NERDTree
" vim没有指定文件时自动打开
" autocmd StdinReadPre * let s:std_in=1
" autocmd VimEnter * if argc() == 0 && !exists("s:std_in") | NERDTree | endif

"ctrlp 配置
let g:NERDTreeChDirMode = 2 
let g:ctrlp_working_path_mode = 'rw' 
let g:ctrlp_max_depth = 40
let g:ctrlp_max_files = 50000
let g:ctrlp_custom_ignore = '\v[\/]\.(git|hg|svn)$'

"ag 配置
let g:ag_prg="ag --vimgrep --smart-case --column"
let g:ag_highlight=1

" 语法高亮
syntax enable
"set background=light
"colorscheme solarized

" 编码格式
set expandtab " 把tab转换成空格
set tabstop=4 " 空格数量是4
set shiftwidth=4 " 自动缩进的宽度

" 编译单个文件
nmap <F11> :call DoOneFileCompile()<CR>
imap <F11> <ESC>:w<CR>:call DoOneFileCompile()<CR>
function DoOneFileCompile()
    if &filetype!="c"
        echo "this file isn't lpc file，can not be compiled.."
        return
    endif

    let path=getcwd()
    let num=match(path, "logic")
    let path=strpart(path, num)
    if path!="logic"
        echo "please first move to root logic dir，then push F11 to compile.."
        return
    endif
    
    let curpath=getcwd()
    let fullpath=expand("%:p")
    let len=strlen(curpath) 
    let filepath=strpart(fullpath, len)
    echo filepath
    execute "!./../engine/engine.nostrip -r ".filepath
    "execute "!./../engine/engine -r ".filepath
endfunction

" 热更单个文件
nmap <F12> :call DoOneFileUpdate()<CR>
imap <F12> <ESC>:w<CR>:call DoOneFileUpdate()<CR>
function DoOneFileUpdate()
    if &filetype!="c"
        echo "this file isn't lpc file，can not be compiled.."
        return
    endif

    let path=getcwd()
    let num=match(path, "logic")
    let path=strpart(path, num)
    if path!="logic"
        echo "please first move to root logic dir，then push F11 to compile.."
        return
    endif
    
    let curpath=getcwd()
    let fullpath=expand("%:p")
    let len=strlen(curpath) 
    let filepath=strpart(fullpath, len)
    execute "!echo ".filepath." > etc/autoupdate"
endfunction


" to make ctrlp more faster
let g:ctrlp_use_caching = 0
if executable('ag')
    set grepprg=ag\ --nogroup\ --nocolor
    let g:ctrlp_user_command = 'ag %s -l --nocolor -g ""'
else
    let g:ctrlp_user_command = ['.git', 'cd %s && git ls-files . -co --exclude-standard', 'find %s -type f']
    let g:ctrlp_prompt_mappings = {
    \ 'AcceptSelection("e")': ['<space>', '<cr>', '<2-LeftMouse>'],
    \ }
endif

set nu
set nobackup
set cursorline
set ruler
set autoindent

"powerline插件
"let g:Powerline_colorscheme ='solarized'
let g:Powerline_colorscheme ='solarized256'
"默认显示状态栏
set laststatus=2

set showcmd

"grep.vim插件
"let Grep_Default_Filelist = '*.[ch]'
let Grep_Default_Options = '-inR --color=auto'
let Grep_Skip_Dirs = '.svn .git '
let Grep_Skip_Files = '*.bak *~ *.swp'

" 替换函数。参数说明：
" confirm：是否替换前逐一确认
" wholeword：是否整词匹配
" replace：被替换字符串
function! Replace(confirm, wholeword, replace)
    wa
    let flag = ''
    if a:confirm
        let flag .= 'gec'
    else
        let flag .= 'ge'
    endif
    let search = ''
    if a:wholeword
        let search .= '\<' . escape(expand('<cword>'),'/\.*$^~[') . '\>'
    else
        let search .= expand('<cword>')
    endif
    let replace = escape(a:replace,'/\&~')
    execute 'argdo %s/' . search . '/' . replace . '/' . flag . '| update'
endfunction
" 不确认、非整词
nnoremap <Leader>R :call Replace(0, 0, input('Replace '.expand('<cword>').'with: '))<CR>
" 不确认、整词
nnoremap <Leader>rw :call Replace(0, 1, input('Replace '.expand('<cword>').'with: '))<CR>
" 确认、非整词
nnoremap <Leader>rc :call Replace(1, 0, input('Replace '.expand('<cword>').'with: '))<CR>
" 确认、整词
nnoremap <Leader>rcw :call Replace(1, 1, input('Replace'.expand('<cword>').' with: '))<CR>
nnoremap <Leader>rwc :call Replace(1, 1, input('Replace'.expand('<cword>').' with: '))<CR>

" 包含目录
" .vim/bundle/ultisnips/mysnippets/c.snippets
let g:UltiSnipsSnippetDirectories=["mysnippets"]

colorscheme desert
