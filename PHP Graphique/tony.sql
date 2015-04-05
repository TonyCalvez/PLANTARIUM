-- phpMyAdmin SQL Dump
-- version 4.1.14
-- http://www.phpmyadmin.net
--
-- Client :  127.0.0.1
-- Généré le :  Jeu 19 Mars 2015 à 20:18
-- Version du serveur :  5.6.17
-- Version de PHP :  5.5.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de données :  `tony`
--

-- --------------------------------------------------------

--
-- Structure de la table `donnees`
--

CREATE TABLE IF NOT EXISTS `donnees` (
  `date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `temp` varchar(255) NOT NULL,
  `hygro` varchar(255) NOT NULL,
  UNIQUE KEY `date` (`date`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Contenu de la table `donnees`
--

INSERT INTO `donnees` (`date`, `temp`, `hygro`) VALUES
('2015-03-18 19:49:29', '22.20', '53.10'),
('2015-03-18 19:49:31', '22.20', '53.10'),
('2015-03-18 19:49:32', '22.20', '53.10'),
('2015-03-18 19:49:33', '22.20', '53.10'),
('2015-03-18 19:51:27', '23.00', '75.90'),
('2015-03-18 19:51:46', '22.80', '70.80'),
('2015-03-19 19:17:34', '0.00', '0.00');

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
