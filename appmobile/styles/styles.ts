import { 
    StyleSheet, 
  } from 'react-native';

export const styles = StyleSheet.create({

    title: {
        fontWeight: 'bold',
        marginBottom: 10,
        textAlign: 'left',
        color: '#911111',
        fontFamily: 'Arial',
        justifyContent: 'flex-start'
    },
    container: {
      flex: 1,
      justifyContent: 'center',
      alignItems: 'center',
      padding: 20,
      backgroundColor: '#f0f0f0',
    },
    rowContainer: {
      flexDirection: 'row',
      marginVertical: 10,
      justifyContent: 'space-between',
      
    },
    rightSideContent: {
      position: 'absolute',
      right: 16,             // Marge par rapport au bord droit
      top: '50%',            // Aligné verticalement avec le bouton du centre
      transform: [           // Ajustement vertical
        { translateY: -20 }  // À ajuster selon la taille du bouton
      ],
      alignItems: 'center',
    },
    text: {
      marginTop: 8,
      textAlign: 'center',
    },
    button: {
      backgroundColor: '#e0e0e0',
      padding: 15,
      margin: 5,
      borderRadius: 10,
      minWidth: 50,
      alignItems: 'center'
    },
    selectedButton: {
      backgroundColor: '#4CAF50'
    },
    buttonText: {
      color: 'black',
      fontWeight: 'bold'
    },
    directionalContainer: {
      marginVertical: 10,
      width: "100%",

    },
    directionalRow: {
      flexDirection: 'row',
      justifyContent: 'center',

    },
    directionalMiddleRow: {
      flexDirection: 'row',
      justifyContent: 'center',
      gap: 50,
    },
    directionalButton: {
      backgroundColor: '#e0e0e0',
      padding: 15,
      margin: 5,
      borderRadius: 10,
      width: 100,
      height: 50,
      alignItems: 'center',
      justifyContent: 'center'
    },
    // Nouveaux styles pour les messages reçus
    receivedMessagesContainer: {
      marginTop: 20,
      width: '100%',
      maxHeight: 200,
      borderTopWidth: 1,
      borderTopColor: '#ccc',
      paddingTop: 10,
    },
    receivedMessagesTitle: {
      fontSize: 16,
      fontWeight: 'bold',
      marginBottom: 10,
      textAlign: 'center',
    },
    receivedMessagesList: {
      maxHeight: 150,
    },
    receivedMessageText: {
      backgroundColor: '#f0f0f0',
      padding: 5,
      marginVertical: 2,
      borderRadius: 5,
    },
    clearButton: {
      alignSelf: 'flex-end',
      marginRight: 10,
      backgroundColor: '#ff4500',
      padding: 5,
      borderRadius: 5,
    },
    clearButtonText: {
      color: 'white',
      fontSize: 12,
    },
    pressReleaseButton: {
      backgroundColor: '#007bff',
      padding: 15,
      margin: 10,
      borderRadius: 10,
      minWidth: 100,
      alignItems: 'center'
    },
  });