<?php
$dtb_hote = 'localhost'; //l'ip du raspi, localhost vu que le serveur bdd est le m�me que le serveur php
$dtb_utilisateur = 'tony'; // un identifiant que tu peux cr�er dans phpmyadmin (ajouter un utilisateur)
$dtb_motdepasse = 'calvez'; //le mot de passe qui va avec
$dtb_basededonnee = 'tony'; // la base de donn�e. Alors soit tu la fais manuellement , mais moi j'ai trich�, tu peux la g�n�rer en m�me temps que l'utilisateur
$dtb_table = 'donnees'; //le nom de la table (j'ai trich�, tu pourras toi aussi, ce fichier suffit de l'importer)

$mysqli = new mysqli($dtb_hote, $dtb_utilisateur, $dtb_motdepasse, "test");

$mysqli->select_db($dtb_basededonnee);

// Si acces au lien api.php?temp=[data]&hygro=[data], alors mettre dans la BDD
if(isset($_GET['cron'])){
	//On r�cupere le document XML g�n�r� par l'Arduino
$doc = simplexml_load_file('http://192.168.1.50/');//Doit correspondre a l'adresse de l'Ethernet Shield
 
//On boucle sur tous les �l�ments du document XML
foreach($doc as $elem)
{
    //On r�cup�re les attributs de l'�lement notamment le nom du capteur

    $attributes = $elem->attributes();
    $name = $attributes["name"];
     
    //On r�cupere les valeurs des capteurs (temp�rature/humidit�)

    $enfants = $elem->children();
    $temp = $enfants[0];
    $hygro = $enfants[1];
		
	$mysqli->query('INSERT INTO '.$dtb_table.'(temp, hygro) VALUES("'.$temp.'", "'.$hygro.'")'); // On insere tout �a dans la BDD
     
}
	
	
}

//Si demande du fichier CSV par le graphique
if(isset($_GET['csv'])){ 
	echo ($_GET['csv'].'("Day,Temp\351rature (en \260C),Hygrom\351trie (en pourcentage)\n');
	
$requete = $mysqli->query("SELECT * FROM ".$dtb_table); // Requete sur la table des donn�es
while($variable_tableau = mysqli_fetch_assoc($requete)) //Recherche de tous les r�sultats
    { 
		echo $variable_tableau['date'].','.$variable_tableau['temp'].','.$variable_tableau['hygro'].'\n';
	}
	 echo ('");');
}
?>