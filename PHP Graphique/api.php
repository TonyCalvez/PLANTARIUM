<?php
$dtb_hote = 'localhost'; //l'ip du raspi, localhost vu que le serveur bdd est le même que le serveur php
$dtb_utilisateur = 'tony'; // un identifiant que tu peux créer dans phpmyadmin (ajouter un utilisateur)
$dtb_motdepasse = 'calvez'; //le mot de passe qui va avec
$dtb_basededonnee = 'tony'; // la base de donnée. Alors soit tu la fais manuellement , mais moi j'ai triché, tu peux la générer en même temps que l'utilisateur
$dtb_table = 'donnees'; //le nom de la table (j'ai triché, tu pourras toi aussi, ce fichier suffit de l'importer)

$mysqli = new mysqli($dtb_hote, $dtb_utilisateur, $dtb_motdepasse, "test");

$mysqli->select_db($dtb_basededonnee);

// Si acces au lien api.php?temp=[data]&hygro=[data], alors mettre dans la BDD
if(isset($_GET['cron'])){
	//On récupere le document XML généré par l'Arduino
$doc = simplexml_load_file('http://192.168.1.50/');//Doit correspondre a l'adresse de l'Ethernet Shield
 
//On boucle sur tous les éléments du document XML
foreach($doc as $elem)
{
    //On récupére les attributs de l'élement notamment le nom du capteur

    $attributes = $elem->attributes();
    $name = $attributes["name"];
     
    //On récupere les valeurs des capteurs (température/humidité)

    $enfants = $elem->children();
    $temp = $enfants[0];
    $hygro = $enfants[1];
		
	$mysqli->query('INSERT INTO '.$dtb_table.'(temp, hygro) VALUES("'.$temp.'", "'.$hygro.'")'); // On insere tout ça dans la BDD
     
}
	
	
}

//Si demande du fichier CSV par le graphique
if(isset($_GET['csv'])){ 
	echo ($_GET['csv'].'("Day,Temp\351rature (en \260C),Hygrom\351trie (en pourcentage)\n');
	
$requete = $mysqli->query("SELECT * FROM ".$dtb_table); // Requete sur la table des données
while($variable_tableau = mysqli_fetch_assoc($requete)) //Recherche de tous les résultats
    { 
		echo $variable_tableau['date'].','.$variable_tableau['temp'].','.$variable_tableau['hygro'].'\n';
	}
	 echo ('");');
}
?>