import QtQuick 2.0
import QtQuick.Particles 2.0

AbstractPage {
    id: root

    focus: true

    ParticleSystem {
        id: particlesSys
    }

    Image {
        id: picture
        anchors.centerIn: parent
        source: "qrc:///images/avatar2.png"
    }

    CustomParticle {
        id: customParticle

        system: particlesSys

        property real maxWidth: width
        property real maxHeight: height

        anchors.fill: picture

        anchors.centerIn: parent

        ShaderEffectSource {
            id: pictureSource
            sourceItem: picture
            hideSource: false
        }

        ShaderEffectSource {
            id: particleSource
            sourceItem: particle
            hideSource: true
        }
        Image {
            id: particle
            source: "qrc:///particleresources/fuzzydot.png"
        }
        //! [vertex]
        vertexShader:"
            uniform highp float maxWidth;
            uniform highp float maxHeight;
            varying highp vec2 fTex2;
            varying lowp float fFade;
            uniform lowp float qt_Opacity;

            void main() {

                fTex2 = vec2(qt_ParticlePos.x, qt_ParticlePos.y);
                //Uncomment this next line for each particle to use full texture, instead of the solid color at the center of the particle.
                //fTex2 = fTex2 + ((- qt_ParticleData.z / 2. + qt_ParticleData.z) * qt_ParticleTex); //Adjusts size so it's like a chunk of image.
                fTex2 = fTex2 / vec2(maxWidth, maxHeight);
                highp float t = (qt_Timestamp - qt_ParticleData.x) / qt_ParticleData.y;
                fFade = min(t*4., (1.-t*t)*.75) * qt_Opacity;
                defaultMain();
            }
        "
        //! [vertex]
        property variant particleTexture: particleSource
        property variant pictureTexture: pictureSource
        //! [fragment]
        fragmentShader: "
            uniform sampler2D particleTexture;
            uniform sampler2D pictureTexture;
            varying highp vec2 qt_TexCoord0;
            varying highp vec2 fTex2;
            varying lowp float fFade;
            void main() {
                gl_FragColor = texture2D(pictureTexture, fTex2) * texture2D(particleTexture, qt_TexCoord0).w * fFade;
        }"
        //! [fragment]
    }

    Emitter {
        id: emitter

        anchors.fill: customParticle
        system: particlesSys
        enabled: false
        lifeSpan: 5000
        maximumEmitted: 10000
        size: 10
        acceleration: PointDirection { xVariation: 15; yVariation: 15 }
    }

    onPlay: {
        emitter.burst(10000)
        pictureSource.hideSource = true
    }
}
