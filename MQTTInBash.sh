DuckDNS_domain="INSERT_DOMAIN_HERE"
DuckDNS_token="INSERT_TOKEN_HERE"

update_dns(){
     curr_ip=$(curl -s https://ipinfo.io/ip)
     curl -k "https://www.duckdns.org/update?domains=${DuckDNS_domain}&token=${DuckDNS_token}&ip=${curr_ip}"
}
mqttTopic="MudGameServ"
update_dns

while true; do
        read -p "EnterPath: /" pathVar
        desc=$(<$pathVar/desc.txt)
        if [[ -d $pathVar ]]; then
                cd ${pathVar}
                mosquitto_pub -h $DuckDNS_domain -t $mqttTopic -m "$desc"
                sleep 1
        else
                mosquitto_pub -h $DuckDNS_domain -t $mqttTopic -m "That path doesn't exist"
                sleep 2
                mosquitto_pub -h $DuckDNS_domain -t $mqttTopic -m "$desc"       
        fi
done