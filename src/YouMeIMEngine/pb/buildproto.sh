protos=`ls | grep "\.proto"`

echo $protos

for file in $protos;do
	./protobufutil -I=. --cpp_out=.  $file
done

find . -name "*.h" -or -name "*.cc" | xargs sed -i "" -e "s/google::/youmecommon::/g"
find . -name "*.h" -or -name "*.cc" | xargs sed -i "" -e "s/namespace\ google/namespace\ youmecommon/g"
find . -name "*.h" -or -name "*.cc" | xargs sed -i "" -e "s/\"youme_comm.pb.h\"/<YouMeCommon\/pb\/youme_comm.pb.h\>/g"

#删除common相关的文件
#rm youme_comm.*

