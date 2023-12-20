DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

rm -fr $DIR/src/

mkdir src
mkdir src/framework
mkdir src/cocos

cp -rf "$DIR"/quick/framework/ "$DIR"/src/framework/
cp -rf "$DIR"/quick/cocos/ "$DIR"/src/cocos/

sh $QUICK_V3_ROOT/quick/bin/compile_scripts.sh -i ./src/ -o ./framework_precompiled.zip -b 32
sh $QUICK_V3_ROOT/quick/bin/compile_scripts.sh -i ./src/ -o ./framework_precompiled64.zip -b 64

rm -fr $DIR/src/