// import React, { useState, useEffect } from 'react';
// import TreeManagement from './components/TreeManagement';

// const stateMapping = {
//     "61": "TEMP_MODE",
//     "62": "SOIL_MOISTURE_MODE",
//     "63": "SATURDAY_MODE",
//     "64": "MANUAL_MODE"
// };

// const App = () => {
//     const [state, setState] = useState('');
//     const [stateData, setStateData] = useState({});
//     const [editedData, setEditedData] = useState({});

//     // שליפת מצב המערכת
//     useEffect(() => {
//         fetch('http://localhost:3011/esp/state')
//             .then(res => res.json())
//             .then(data => setState(data.CurrentStatus))
//             .catch(err => console.error('Error fetching state:', err));
//     }, []);

//     // שליפת נתונים לפי מצב נוכחי
//     useEffect(() => {
//         if (state) {
//             const stateKey = stateMapping[state] || state;
//             fetch(`http://localhost:3011/esp/dataMode?state=${encodeURIComponent(stateKey)}`)
//                 .then(res => {
//                     if (!res.ok) {
//                         throw new Error(`HTTP error! Status: ${res.status}`);
//                     }
//                     return res.json();
//                 })
//                 .then(data => {
//                     setStateData(data);
//                     setEditedData(data);
//                 })
//                 .catch(err => console.error('Error fetching state data:', err));
//         }
//     }, [state]);

//     // שינוי מצב המערכת
//     const updateState = (newState) => {
//         fetch(`http://localhost:3011/esp/state=${encodeURIComponent(newState)}`)
//             .then(res => res.json())
//             .then(data => setState(newState))
//             .catch(err => console.error('Error updating state:', err));
//     };

//     // עדכון ערכים
//     const handleInputChange = (key, value) => {
//         setEditedData(prev => ({ ...prev, [key]: value }));
//     };

//     // שליחת הנתונים המעודכנים לשרת
//     const saveChanges = () => {
//         fetch(`http://localhost:3011/esp/updateState?state=${encodeURIComponent(state)}`, {
//             method: 'POST',
//             headers: { 'Content-Type': 'application/json' },
//             body: JSON.stringify(editedData)
//         })
//         .then(res => res.json())
//         .then(data => {
//             console.log("Updated state data successfully", data);
//             setStateData(editedData);
//         })
//         .catch(err => console.error('Error updating state data:', err));
//     };

//     return (
//         <div className="p-6 max-w-3xl mx-auto bg-white shadow-lg rounded-lg">
//             <h1 className="text-2xl font-bold text-gray-800 mb-4">ניהול מערכת</h1>
//             <p>מצב נוכחי: <strong>{state}</strong></p>
//             <button className="bg-blue-500 text-white px-4 py-2 rounded mt-2" onClick={() => updateState('64')}>עבור למצב ידני</button>
//             <button className="bg-green-500 text-white px-4 py-2 rounded mt-2 ml-2" onClick={() => updateState('61')}>עבור למצב טמפרטורה</button>
//             <button className="bg-yellow-500 text-white px-4 py-2 rounded mt-2 ml-2" onClick={() => updateState('62')}>עבור למצב לחות קרקע</button>
//             <button className="bg-red-500 text-white px-4 py-2 rounded mt-2 ml-2" onClick={() => updateState('63')}>עבור למצב שבת</button>

//             <h2 className="text-xl font-semibold text-gray-700 mt-6">עריכת נתונים</h2>
//             {Object.keys(editedData).map((key) => (
//                 <div key={key} className="mb-2">
//                     <label className="block text-gray-700">{key}</label>
//                     <input 
//                         type="text" 
//                         value={editedData[key]} 
//                         onChange={(e) => handleInputChange(key, e.target.value)} 
//                         className="border px-2 py-1 w-full rounded"
//                     />
//                 </div>
//             ))}
//             <button className="bg-blue-500 text-white px-4 py-2 rounded mt-2" onClick={saveChanges}>שמור שינויים</button>

//             <TreeManagement />
//         </div>
//     );
// };

// export default App;
import React, { useState, useEffect } from 'react';

import TreeManagement from './components/TreeManagement';



const stateMapping = {

    "61": "TEMP_MODE",

    "62": "SOIL_MOISTURE_MODE",

    "63": "SATURDAY_MODE",

    "64": "MANUAL_MODE"

};



const App = () => {

    const [state, setState] = useState('');

    const [stateData, setStateData] = useState({});

    const [editedData, setEditedData] = useState({});



    // שליפת מצב המערכת

    useEffect(() => {

        fetch('http://localhost:3011/esp/state')

            .then(res => res.json())

            .then(data => setState(data.CurrentStatus))

            .catch(err => console.error('Error fetching state:', err));

    }, []);



    // שליפת נתונים לפי מצב נוכחי

    useEffect(() => {

        if (state) {

            const stateKey = stateMapping[state] || state;

            fetch(`http://localhost:3011/esp/dataMode?state=${encodeURIComponent(stateKey)}`)

                .then(res => {

                    if (!res.ok) {

                        throw new Error(`HTTP error! Status: ${res.status}`);

                    }

                    return res.json();

                })

                .then(data => {

                    setStateData(data);

                    setEditedData(data);

                })

                .catch(err => console.error('Error fetching state data:', err));

        }

    }, [state]);



    // שינוי מצב המערכת

    const updateState = (newState) => {

        fetch(`http://localhost:3011/esp/state=${encodeURIComponent(newState)}`)

            .then(res => res.json())

            .then(data => setState(newState))

            .catch(err => console.error('Error updating state:', err));

    };



    // עדכון ערכים

    const handleInputChange = (key, value) => {

        setEditedData(prev => ({ ...prev, [key]: value }));

    };



    // שליחת הנתונים המעודכנים לשרת

    const saveChanges = () => {
        const requestBody = {
            state: state, // שולח את המצב הנוכחי
            updatedData: editedData // שולח את הנתונים המעודכנים
        };
    
        console.log("📌 שולח נתונים לשרת:", requestBody);
    
        fetch(`http://localhost:3011/esp/updateInsideInformation`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(requestBody)
        })
        .then(res => {
            if (!res.ok) {
                return res.json().then(error => { throw new Error(error.error || "Unknown error"); });
            }
            return res.json();
        })
        .then(data => {
            console.log("✅ עדכון הצליח:", data);
            setStateData(editedData);
        })
        .catch(err => console.error('❌ שגיאה בעדכון:', err.message));
    };
    

    return (

        <div className="p-6 max-w-3xl mx-auto bg-white shadow-lg rounded-lg">

            <h1 className="text-2xl font-bold text-gray-800 mb-4">ניהול מערכת</h1>

            <p>מצב נוכחי: <strong>{state}</strong></p>

            <button className="bg-blue-500 text-white px-4 py-2 rounded mt-2" onClick={() => updateState('64')}>עבור למצב ידני</button>

            <button className="bg-green-500 text-white px-4 py-2 rounded mt-2 ml-2" onClick={() => updateState('61')}>עבור למצב טמפרטורה</button>

            <button className="bg-yellow-500 text-white px-4 py-2 rounded mt-2 ml-2" onClick={() => updateState('62')}>עבור למצב לחות קרקע</button>

            <button className="bg-red-500 text-white px-4 py-2 rounded mt-2 ml-2" onClick={() => updateState('63')}>עבור למצב שבת</button>



            <h2 className="text-xl font-semibold text-gray-700 mt-6">עריכת נתונים</h2>

            {Object.keys(editedData).map((key) => (

                <div key={key} className="mb-2">

                    <label className="block text-gray-700">{key}</label>

                    <input

                        type="text"

                        value={editedData[key]}

                        onChange={(e) => handleInputChange(key, e.target.value)}

                        className="border px-2 py-1 w-full rounded"

                    />

                </div>

            ))}

            <button className="bg-blue-500 text-white px-4 py-2 rounded mt-2" onClick={saveChanges}>שמור שינויים</button>



            <TreeManagement />

        </div>

    );

};



export default App;