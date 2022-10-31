### Initialize
To initialize your repo do:

  * make fork from `https://github.com/ostis-ai/scl-machine`
  * clone your fork to your machine and prepare:
  ```sh
  git clone git@github.com:username/scl-machine.git
  cd scl-machine
  git remote add upstream git@github.com:ostis-ai/scl-machine.git
  ```

### Update
To update your main branch from `upstream` use:
```sh
git fetch upstream
git checkout main
git merge upstream/main
git push origin main
```

### Pre-commit
To setup `pre-commit` run:
```sh
cd scl-machine
pre-commit install
```
