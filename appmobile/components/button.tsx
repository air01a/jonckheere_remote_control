import React from 'react';
import { View, TouchableOpacity, Text } from 'react-native';
import { styles } from '../styles/styles'; // Adjust the import path as necessary

const Button = ({
    options, 
    selectedValue=null, 
    onPress
  }: {
    options: string[], 
    selectedValue?: string | null, 
    onPress: (value: any) => void
  }) => (
    <View style={styles.rowContainer}>
      {options.map((option) => (
        <TouchableOpacity
          key={option}
          style={[
            styles.button, 
            selectedValue === option && styles.selectedButton
          ]}
          onPress={() => onPress(option)}
        >
          <Text style={styles.buttonText}>{option}</Text>
        </TouchableOpacity>
      ))}
    </View>
  );
  
  export default Button;