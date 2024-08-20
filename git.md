# git 使用

## options

--depth=                            克隆深度
                                    git clone --depth=1 [xxx.git]       用于指定克隆深度，为1即表示只克隆最近一次commit

## 常用的命令

git init                            初始化本地git环境
git clone [xxx.git]                 克隆一份代码到本地仓库
git pull                            把远程库的内容更新到本地并合并到当前分支(等价于 git fetch  git merge)
git add <object file>               把本地的修改加到stage中
git commit -m "message"             把stage中的修改提交到本地库, message是这次提交的说明
git push                            把本地库的修改提交到远程库中
git fetch                           把远程库的代码更新到本地库
git merge <name>                    合并某分支到当前分支
git status                          查看当前分支有哪些修改
git diff                            查看哪些文件冲突
git log                             查看当前分支上面的日志信息
git reset --hard commit_id          版本回退

## 解决冲突

用git diff 查看哪些文件冲突，有冲突的会提示：
++<<<<<<< HEAD
++<<<<<<< new_branch
修改完冲突后 git merge 合并代码,再 git pull 同步远程仓库代码,再 git push 提交本地仓库代码到远程仓库

## 操作分支

git branch                          查看本地分支
git branch -r/-a                    查看远程分支/全部分支
git branch <name>                   创建分支
git branch -d <name>                删除分支
git branch -D <name>                丢弃一个没有被合并过的分支
git checkout <name>                 切换分支
git checkout -b <name>              创建并切换分支
git log --graph                     查看分支合并图
git stash                           当前工作现场"储藏"起来(在当前需要保存的分支进行操作)

## 标签

git tag                             查看标签
git tag '0.0.1'                     添加标签
git tag -d '0.0.1'                  删除标签

## 配置信息

.git 文件夹下是所有 git 的相关信息, .git 文件本身是隐藏的

.git/config                         git配置信息
下面这个参数是保存用户名和密码
[credential]
    helper = store

配置全局用户和email
git config --global user.name dioq
git config --global user.email <ptttqq@gmail.com>

清除缓存(.git/config)
git config --local --unset credential.helper
git config --global --unset credential.helper
git config --system --unset credential.helper

大文件
brew install git-lfs   # install git-lfs
git lfs install        # Verify that the installation was successful

git lfs track "*.psd"  # add large file
git add .gitattributes # Now make sure .gitattributes is tracked
